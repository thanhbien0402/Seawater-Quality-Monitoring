import { zodResolver } from '@hookform/resolvers/zod'
import { useForm } from 'react-hook-form'
import * as z from 'zod'

import {
  Form,
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage
} from '@/components/ui/form'
import { Input } from '@/components/ui/input'
import { useToast } from '@/hooks/use-toast'
import { Button } from '@/components/ui/button'
import { cn } from '@/lib/utils'
import { Icons } from '@/components/icons'
import { signInWithEmailAndPassword } from '@/actions'
import { useTransition } from 'react'
import { redirect } from 'next/navigation'

const FormSchema = z.object({
  email: z.string().email(),
  password: z.string().min(1, {
    message: 'Password is required.'
  })
})

export default function SignInForm() {
  const { toast } = useToast()
  const [isPending, startTransition] = useTransition()

  const form = useForm<z.infer<typeof FormSchema>>({
    resolver: zodResolver(FormSchema),
    defaultValues: {
      email: '',
      password: ''
    }
  })

  function onSubmit(data: z.infer<typeof FormSchema>) {
    console.log('data', data)
    startTransition(async () => {
      const result = await signInWithEmailAndPassword(data)
      console.log('result', result)
      const { error } = result

      if (error?.status) {
        toast({
          variant: 'destructive',
          title: 'You submitted the following values:',
          description: (
            <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
              <code className="text-white">{error.code}</code>
            </pre>
          )
        })
      } else {
        console.log('succes')
        toast({
          title: 'You submitted the following values:',
          description: (
            <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
              <code className="text-white">Successfully Login</code>
            </pre>
          )
        })
        redirect('/')
      }
    })
  }

  return (
    <Form {...form}>
      <form onSubmit={form.handleSubmit(onSubmit)} className="w-full space-y-6">
        <FormField
          control={form.control}
          name="email"
          render={({ field }) => (
            <FormItem>
              <FormLabel className="flex justify-start">Email</FormLabel>
              <FormControl>
                <Input
                  className="bg-white"
                  placeholder="example@gmail.com"
                  {...field}
                  type="email"
                  onChange={field.onChange}
                />
              </FormControl>
              <FormMessage className="flex justify-start" />
            </FormItem>
          )}
        />
        <FormField
          control={form.control}
          name="password"
          render={({ field }) => (
            <FormItem>
              <FormLabel className="flex justify-start">Password</FormLabel>
              <FormControl>
                <Input
                  className="bg-white"
                  placeholder="password"
                  {...field}
                  type="password"
                  onChange={field.onChange}
                />
              </FormControl>

              <FormMessage className="flex justify-start" />
            </FormItem>
          )}
        />
        <Button type="submit" className="w-full flex gap-2">
          Sign In
          <Icons.spinner
            className={cn('animate-spin', { hidden: !isPending })}
          />
        </Button>
      </form>
    </Form>
  )
}
