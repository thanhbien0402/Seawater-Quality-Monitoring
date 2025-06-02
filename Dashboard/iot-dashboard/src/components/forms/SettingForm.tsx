'use client'
import { startTransition, useTransition } from 'react'
import { Icons } from '../icons'
import { Button } from '../ui/button'
import { Input } from '../ui/input'
import { createSetting, sendDataTb } from '@/actions'
import { useToast } from '@/hooks/use-toast'
import * as z from 'zod'
import {
  Form,
  FormControl,
  FormDescription,
  FormField,
  FormItem,
  FormLabel,
  FormMessage
} from '../ui/form'
import { cn } from '@/lib/utils'
import { zodResolver } from '@hookform/resolvers/zod'
import { useForm } from 'react-hook-form'

const FormSchema = z.object({
  monitoringTime: z.number().gte(30, {
    message: 'You can only set the monitoring time with value greater or equal 30 seconds'
  })
})

export const SettingForm = ({ deviceId }: { deviceId: string }) => {
  const { toast } = useToast()

  const form = useForm<z.infer<typeof FormSchema>>({
    resolver: zodResolver(FormSchema),
    defaultValues: {
      monitoringTime: 30
    }
  })

  async function onSubmit(data: z.infer<typeof FormSchema>) {

    const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))

    const { monitoringTime } = data

    try {
      const res = await sendDataTb('monitoring', 'control', token, deviceId, monitoringTime)

      toast({
        title: "You submitted the following values:",
        description: (
          <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
            <p className="text-white">Submitted Successfully</p>
          </pre>
        ),
      })
    } catch (error) {
      console.log('error >>>>>', error)
    }
    form.reset()
  }
  return (
    <Form {...form}>
      <form
        onSubmit={form.handleSubmit(onSubmit)}
        className="w-full space-y-6 px-3 md:px-0 flex gap-2"
      >
        <div className='flex-[3]'>
          <FormField
            control={form.control}
            name="monitoringTime"
            render={({ field }) => (
              <FormItem>
                <FormLabel className="flex justify-start">Monitoring Time</FormLabel>
                <FormControl>
                  <Input
                    className="border-blue-950"
                    placeholder="60"
                    {...field}
                    type="number"
                    onChange={(e) => {
                      const v = e.target.value
                      field.onChange(v === '' ? undefined : Number(v))
                    }}
                  />
                </FormControl>
                <FormDescription>
                  A number value representing for how long to monitoring again. For example,
                  '60' seconds
                </FormDescription>
                <FormMessage className="flex justify-start" />
              </FormItem>
            )}
          />
        </div>
        {/* <FormField
          control={form.control}
          name="entityId"
          render={({ field }) => (
            <FormItem>
              <FormLabel className="flex justify-start">EntityId*</FormLabel>
              <FormControl>
                <Input
                  className="border-blue-950"
                  placeholder="784f394c-42b6-435a-983c-b7beff2784f9"
                  {...field}
                  type="string"
                  onChange={field.onChange}
                />
              </FormControl>
              <FormDescription>
                A string value representing the entity id. For example,
                '784f394c-42b6-435a-983c-b7beff2784f9'
              </FormDescription>
              <FormMessage className="flex justify-start" />
            </FormItem>
          )}
        /> */}
        <Button type="submit" className="flex-1 flex gap-2 mt-[22px]">
          Edit
        </Button>
      </form>
    </Form>
  )
}
