import { useState } from 'react'
import { DeviceForm } from '../forms/DeviceForm'
import { Button } from '../ui/button'
import {
    Dialog,
    DialogContent,
    DialogDescription,
    DialogHeader,
    DialogTitle,
    DialogTrigger
} from '../ui/dialog'
import { SquarePlus } from 'lucide-react'

export function DeviceModal() {
    const [isOpen, setIsOpen] = useState<boolean>(false)

    return (
        <Dialog open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DialogTrigger asChild>
                <Button variant="ghost" className="gap-1 w-full justify-between">
                    <p>Add new device</p>
                    <SquarePlus className="h-4 w-4" />
                </Button>
            </DialogTrigger>
            <DialogContent className="sm:max-w-[425px]">
                <DialogHeader>
                    <DialogTitle>Add new device</DialogTitle>
                    <DialogDescription>
                        Add new device information below
                    </DialogDescription>
                </DialogHeader>
                <DeviceForm setIsOpen={setIsOpen} />
            </DialogContent>
        </Dialog>
    )
}
